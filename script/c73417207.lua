--Mara of the Nordic Alfar
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
function c73417207.filter(c,syncard,f)
	return c:IsSetCard(0x42) and c:IsNotTuner() and c:IsCanBeSynchroMaterial(syncard) and (f==nil or f(c))
end
function c73417207.lvtest1(c,lv,g)
	local lv1=c:GetLevel()
	return lv1<lv and g:IsExists(c73417207.lvtest2,1,c,lv-lv1)
end
function c73417207.lvtest2(c,lv2)
	return c:GetLevel()==lv2
end
function c73417207.target(e,syncard,f,minc)
	if minc>2 then return false end
	local lv=syncard:GetLevel()-e:GetHandler():GetLevel()
	if lv<=0 then return false end
	local g=Duel.GetMatchingGroup(c73417207.filter,syncard:GetControler(),LOCATION_HAND,0,nil,syncard,f)
	return g:GetCount()>=2 and g:IsExists(c73417207.lvtest1,1,nil,lv,g)
end
function c73417207.operation(e,tp,eg,ep,ev,re,r,rp,syncard,f,minc)
	local lv=syncard:GetLevel()-e:GetHandler():GetLevel()
	local g=Duel.GetMatchingGroup(c73417207.filter,tp,LOCATION_HAND,0,nil,syncard,f)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
	local g1=g:FilterSelect(tp,c73417207.lvtest1,1,1,nil,lv,g)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
	local g2=g:FilterSelect(tp,c73417207.lvtest2,1,1,g1:GetFirst(),lv-g1:GetFirst():GetLevel())
	g1:Merge(g2)
	Duel.SetSynchroMaterial(g1)
end
