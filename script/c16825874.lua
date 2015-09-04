--エキセントリック・ボーイ
function c16825874.initial_effect(c)
	--synchro custom
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SYNCHRO_MATERIAL_CUSTOM)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetTarget(c16825874.target)
	e1:SetValue(1)
	e1:SetOperation(c16825874.operation)
	c:RegisterEffect(e1)
	--be material
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_BE_MATERIAL)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCondition(c16825874.ccon)
	e2:SetOperation(c16825874.cop)
	c:RegisterEffect(e2)
end
c16825874.tuner_filter=aux.FALSE
function c16825874.filter(c,syncard,tuner,f,lv)
	return c:IsNotTuner() and c:IsCanBeSynchroMaterial(syncard,tuner) and (f==nil or f(c)) and c:GetLevel()==lv
end
function c16825874.target(e,syncard,f,minc,maxc)
	if minc>1 then return false end
	local lv=syncard:GetLevel()-e:GetHandler():GetLevel()
	if lv<=0 then return false end
	return Duel.IsExistingMatchingCard(c16825874.filter,syncard:GetControler(),LOCATION_HAND,0,1,nil,syncard,e:GetHandler(),f,lv)
end
function c16825874.operation(e,tp,eg,ep,ev,re,r,rp,syncard,f,minc,maxc)
	local lv=syncard:GetLevel()-e:GetHandler():GetLevel()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
	local g=Duel.SelectMatchingCard(tp,c16825874.filter,tp,LOCATION_HAND,0,1,1,nil,syncard,e:GetHandler(),f,lv)
	Duel.SetSynchroMaterial(g)
end
function c16825874.ccon(e,tp,eg,ep,ev,re,r,rp)
	return r==REASON_SYNCHRO
end
function c16825874.cop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local rc=c:GetReasonCard()
	--leave redirect
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_LEAVE_FIELD_REDIRECT)
	e1:SetValue(LOCATION_REMOVED)
	e1:SetReset(RESET_EVENT+0x7e0000)
	rc:RegisterEffect(e1)
	--cannot trigger
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_TRIGGER)
	e2:SetReset(RESET_EVENT+0x1fe0000)
	rc:RegisterEffect(e2)
	--disable
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_DISABLE)
	e3:SetReset(RESET_EVENT+0x1fe0000)
	rc:RegisterEffect(e3)
end
