--氷結界の虎将 ライホウ
function c81275309.initial_effect(c)
	--summon,flip
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_CHAIN_SOLVING)
	e1:SetOperation(c81275309.handes)
	c:RegisterEffect(e1)
end
c81275309[0]=0
function c81275309.handes(e,tp,eg,ep,ev,re,r,rp)
	local loc,id=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION,CHAININFO_CHAIN_ID)
	if ep==tp or loc~=LOCATION_MZONE or id==c81275309[0] or not re:IsActiveType(TYPE_MONSTER) then return end
	c81275309[0]=id
	if Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0 and Duel.SelectYesNo(1-tp,aux.Stringid(81275309,0)) then
		Duel.DiscardHand(1-tp,aux.TRUE,1,1,REASON_EFFECT+REASON_DISCARD,nil)
		Duel.BreakEffect()
	else Duel.NegateEffect(ev) end
end
