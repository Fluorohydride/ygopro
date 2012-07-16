--氷結界に住む魔酔虫
function c92065772.initial_effect(c)
	--dis field
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(92065772,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetOperation(c92065772.operation)
	c:RegisterEffect(e1)
end
function c92065772.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
		and Duel.GetLocationCount(1-tp,LOCATION_MZONE)<=0 then return end
	--disable field
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_DISABLE_FIELD)
	e1:SetOperation(c92065772.disop)
	e1:SetReset(RESET_EVENT+0x1ff0000)
	e:GetHandler():RegisterEffect(e1)
end
function c92065772.disop(e,tp)
	local dis1=Duel.SelectDisableField(tp,1,LOCATION_MZONE,LOCATION_MZONE,0)
	return dis1
end
