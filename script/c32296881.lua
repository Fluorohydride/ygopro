--豊穣のアルテミス
function c32296881.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_CHAIN_SOLVED)
	e1:SetRange(LOCATION_MZONE)
	e1:SetOperation(c32296881.drop)
	c:RegisterEffect(e1)
end
function c32296881.drop(e,tp,eg,ep,ev,re,r,rp)
	if not re:GetHandler():IsType(TYPE_COUNTER) then return end
	Duel.Hint(HINT_CARD,0,32296881)
	Duel.BreakEffect()
	Duel.Draw(tp,1,REASON_EFFECT)
end
