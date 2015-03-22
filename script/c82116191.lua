--ギガント・セファロタス
function c82116191.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(82116191,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c82116191.atkcon)
	e1:SetTarget(c82116191.atktg)
	e1:SetOperation(c82116191.atkop)
	c:RegisterEffect(e1)
end
function c82116191.filter(c)
	return c:IsRace(RACE_PLANT) and c:IsPreviousLocation(LOCATION_ONFIELD)
end
function c82116191.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c82116191.filter,1,nil)
end
function c82116191.atktg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) and e:GetHandler():IsFaceup() end
end
function c82116191.atkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(200)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e1)
	end
end
