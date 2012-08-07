--幻影の妖精
function c8687195.initial_effect(c)
	--change target
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(8687195,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_BATTLE_CONFIRM)
	e1:SetCondition(c8687195.condition)
	e1:SetTarget(c8687195.target)
	e1:SetOperation(c8687195.operation)
	c:RegisterEffect(e1)
end
function c8687195.condition(e,tp,eg,ep,ev,re,r,rp)
	return r~=REASON_REPLACE and Duel.GetAttackTarget()==e:GetHandler() and Duel.GetAttacker():IsControler(1-tp)
end
function c8687195.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,nil,tp,LOCATION_MZONE,0,1,1,e:GetHandler())
end
function c8687195.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.ChangeAttackTarget(tc)
	end
end
