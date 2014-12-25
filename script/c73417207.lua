--極星霊スヴァルトアールヴ
function c73417207.initial_effect(c)
	--synchro custom
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SYNCHRO_MATERIAL_CUSTOM)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetTarget(c73417207.target)
	e1:SetValue(1)
	e1:SetOperation(c73417207.operation)
	c:RegisterEffect(e1)
end
c73417207.tuner_filter=aux.FALSE
function c73417207.filter(c,syncard,tuner,f)
	return c:IsSetCard(0x42) and c:IsNotTuner() and c:IsCanBeSynchroMaterial(syncard,tuner) and (f==nil or f(c))
end
function c73417207.target(e,syncard,f,minc,maxc)
	local c=e:GetHandler()
	if minc>2 or maxc<2 then return false end
	local lv=syncard:GetLevel()-c:GetLevel()
	if lv<=0 then return false end
	local g=Duel.GetMatchingGroup(c73417207.filter,syncard:GetControler(),LOCATION_HAND,0,c,syncard,c,f)
	return g:CheckWithSumEqual(Card.GetSynchroLevel,lv,2,2,syncard)
end
function c73417207.operation(e,tp,eg,ep,ev,re,r,rp,syncard,f,minc,maxc)
	local c=e:GetHandler()
	local lv=syncard:GetLevel()-c:GetLevel()
	local g=Duel.GetMatchingGroup(c73417207.filter,syncard:GetControler(),LOCATION_HAND,0,c,syncard,c,f)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
	local sg=g:SelectWithSumEqual(tp,Card.GetSynchroLevel,lv,2,2,syncard)
	Duel.SetSynchroMaterial(sg)
end
