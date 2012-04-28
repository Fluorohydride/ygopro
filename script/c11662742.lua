--ジェルエンデュオ
function c11662742.initial_effect(c)
	--battle indestructable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--double tribute
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e2:SetValue(c11662742.dtcon)
	c:RegisterEffect(e2)
	--selfdes
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetProperty(EFFECT_FLAG_AUXILIARY)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EVENT_DAMAGE)
	e3:SetOperation(c11662742.dmop);
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(11662742,0))
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e4:SetCode(EVENT_BATTLED)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCondition(c11662742.descon);
	e4:SetTarget(c11662742.destg);
	e4:SetOperation(c11662742.desop);
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(11662742,0))
	e5:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e5:SetCode(EVENT_BATTLED)
	e5:SetRange(LOCATION_MZONE)
	e5:SetCondition(c11662742.descon2);
	e5:SetTarget(c11662742.destg);
	e5:SetOperation(c11662742.desop);
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetDescription(aux.Stringid(11662742,0))
	e6:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e6:SetRange(LOCATION_MZONE)
	e6:SetCode(EVENT_DAMAGE)
	e6:SetCondition(c11662742.descon3);
	e6:SetTarget(c11662742.destg);
	e6:SetOperation(c11662742.desop);
	c:RegisterEffect(e6)
end
function c11662742.dtcon(e,c)
	return c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsRace(RACE_FAIRY)
end
function c11662742.dmop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:GetControler()~=ep then return end
	if bit.band(r,REASON_BATTLE)~=0 then
		c:RegisterFlagEffect(11662742,RESET_PHASE+RESET_DAMAGE,0,0)
	elseif Duel.GetCurrentPhase()==PHASE_DAMAGE and not Duel.IsDamageCalculated() then
		c:RegisterFlagEffect(11662743,RESET_PHASE+RESET_DAMAGE,0,1)
	end
end
function c11662742.descon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(11662742)>0
end
function c11662742.descon2(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(11662743)>0
end
function c11662742.descon3(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(r,REASON_EFFECT)~=0 and e:GetHandler():GetControler()==ep
		and (Duel.GetCurrentPhase()~=PHASE_DAMAGE or Duel.IsDamageCalculated())
end
function c11662742.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,e:GetHandler(),1,0,0)
end
function c11662742.desop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.Destroy(e:GetHandler(),REASON_EFFECT)
	end
end
