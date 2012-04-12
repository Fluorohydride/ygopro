--炎龍
function c36750412.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(36750412,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c36750412.atkcon)
	e1:SetOperation(c36750412.atkop)
	c:RegisterEffect(e1)
end
function c36750412.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c36750412.atkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetProperty(EFFECT_FLAG_COPY_INHERIT)
		e1:SetValue(200)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
	end
end
