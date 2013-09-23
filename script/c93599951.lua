--沈黙の邪悪霊
function c93599951.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c93599951.condition)
	e1:SetTarget(c93599951.target)
	e1:SetOperation(c93599951.activate)
	c:RegisterEffect(e1)
end
function c93599951.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer()
end
function c93599951.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local a=Duel.GetAttacker()
	if chk==0 then return a and a:IsCanBeEffectTarget(e) and Duel.IsExistingTarget(Card.IsFaceup,tp,0,LOCATION_MZONE,1,a) end
	Duel.SetTargetCard(a)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=Duel.SelectTarget(tp,Card.IsFaceup,tp,0,LOCATION_MZONE,1,1,a)
	e:SetLabelObject(g:GetFirst())
end
function c93599951.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		if tc:IsDefencePos() then
			Duel.ChangePosition(tc,POS_FACEUP_ATTACK)
		end
		Duel.ChangeAttacker(tc)
	end
end
