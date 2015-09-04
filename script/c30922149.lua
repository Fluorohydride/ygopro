--副作用？
function c30922149.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW+CATEGORY_RECOVER)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTarget(c30922149.target)
	e1:SetOperation(c30922149.activate)
	c:RegisterEffect(e1)
end
function c30922149.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(1-tp,1) end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,1-tp,1)
end
function c30922149.activate(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	if not Duel.IsPlayerCanDraw(p) then return end
	local ct=Duel.GetFieldGroupCount(p,LOCATION_DECK,0)
	local ac=0
	if ct==0 then ac=1 end
	if ct>1 then
		Duel.Hint(HINT_SELECTMSG,p,aux.Stringid(30922149,0))
		if ct==2 then ac=Duel.AnnounceNumber(p,1,2)
		else ac=Duel.AnnounceNumber(p,1,2,3) end
	end
	local dr=Duel.Draw(p,ac,REASON_EFFECT)
	if p~=tp and dr~=0 then
		Duel.BreakEffect()
		Duel.Recover(tp,dr*2000,REASON_EFFECT)
	end
end
