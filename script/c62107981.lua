--E・HERO クノスペ
function c62107981.initial_effect(c)
	--direct attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DIRECT_ATTACK)
	e1:SetCondition(c62107981.atcon)
	c:RegisterEffect(e1)
	--atk/def
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(62107981,0))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_BATTLE_DAMAGE)
	e2:SetCondition(c62107981.adcon)
	e2:SetOperation(c62107981.adop)
	c:RegisterEffect(e2)
	--
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e3:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCondition(c62107981.atcon)
	e3:SetValue(aux.imval1)
	c:RegisterEffect(e3)
end
function c62107981.adcon(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c62107981.adop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() and c:GetDefence()>=100 then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetProperty(EFFECT_FLAG_COPY_INHERIT)
		e1:SetValue(100)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_UPDATE_DEFENCE)
		e2:SetValue(-100)
		c:RegisterEffect(e2)
	end
end
function c62107981.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x3008)
end
function c62107981.atcon(e)
	return Duel.IsExistingMatchingCard(c62107981.filter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,e:GetHandler())
end
