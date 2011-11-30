--活路への希望
function c80036543.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c80036543.condition)
	e1:SetCost(c80036543.cost)
	e1:SetTarget(c80036543.target)
	e1:SetOperation(c80036543.activate)
	c:RegisterEffect(e1)
end
function c80036543.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetLP(tp)<=Duel.GetLP(1-tp)-1000
end
function c80036543.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) end
	Duel.PayLPCost(tp,1000)
end
function c80036543.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c80036543.activate(e,tp,eg,ep,ev,re,r,rp)
	local p1=Duel.GetLP(tp)
	local p2=Duel.GetLP(1-tp)
	local s=p2-p1
	if s<0 then s=p1-p2 end
	local d=math.floor(s/2000)
	Duel.Draw(tp,d,REASON_EFFECT)
end
