--Sin パラレルギア
function c74509280.initial_effect(c)
	--synchro custom
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SYNCHRO_MATERIAL_CUSTOM)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetTarget(c74509280.target)
	e1:SetValue(1)
	e1:SetOperation(c74509280.operation)
	c:RegisterEffect(e1)
end
c74509280.tuner_filter=aux.FALSE
function c74509280.synfilter(c,syncard,tuner,f,lv)
	return c:IsSetCard(0x23) and c:IsCanBeSynchroMaterial(syncard,tuner) and (f==nil or f(c)) and c:GetLevel()==lv
end
function c74509280.target(e,syncard,f,minc)
	local c=e:GetHandler()
	if minc>1 then return false end
	local lv=syncard:GetLevel()-c:GetLevel()
	if lv<=0 then return false end
	return Duel.IsExistingMatchingCard(c74509280.synfilter,syncard:GetControler(),LOCATION_HAND,0,1,nil,syncard,c,f,lv)
end
function c74509280.operation(e,tp,eg,ep,ev,re,r,rp,syncard,f,minc)
	local c=e:GetHandler()
	local lv=syncard:GetLevel()-c:GetLevel()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
	local g=Duel.SelectMatchingCard(tp,c74509280.synfilter,tp,LOCATION_HAND,0,1,1,nil,syncard,c,f,lv)
	Duel.SetSynchroMaterial(g)
end
