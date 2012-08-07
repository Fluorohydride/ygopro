--八汰烏の骸
function c30461781.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c30461781.target)
	e1:SetOperation(c30461781.activate)
	c:RegisterEffect(e1)
end
function c30461781.filter(c)
	return c:IsType(TYPE_SPIRIT) and c:IsFaceup()
end
function c30461781.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	local d=1
	if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>1
		and Duel.IsExistingMatchingCard(c30461781.filter,tp,0,LOCATION_MZONE,1,nil)
		and Duel.SelectOption(tp,aux.Stringid(30461781,0),aux.Stringid(30461781,1))==1 then
		d=2
	end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(d)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,d)
end
function c30461781.activate(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
