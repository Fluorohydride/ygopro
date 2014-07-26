--ゴースト王－パンプキング－
function c29155212.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c29155212.adval)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(29155212,0))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetProperty(EFFECT_FLAG_NO_TURN_RESET)
	e3:SetCondition(c29155212.atkcon)
	e3:SetOperation(c29155212.atkop)
	c:RegisterEffect(e3)
end
function c29155212.filter(c)
	return c:IsFaceup() and c:IsCode(62121)
end
function c29155212.adval(e,c)
	if Duel.IsExistingMatchingCard(c29155212.filter,0,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) then
		return 100+c:GetFlagEffect(29155212)*100
	else
		return c:GetFlagEffect(29155212)*100
	end
end
function c29155212.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp and e:GetHandler():GetFlagEffect(29155212)<=4
end
function c29155212.atkop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	e:GetHandler():RegisterFlagEffect(29155212,RESET_EVENT+0x1ff0000,0,1)
end
