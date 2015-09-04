--モノ・シンクロン
function c56897896.initial_effect(c)
	--synchro custom
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SYNCHRO_MATERIAL_CUSTOM)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetTarget(c56897896.syntg)
	e1:SetValue(1)
	e1:SetOperation(c56897896.synop)
	c:RegisterEffect(e1)
end
c56897896.tuner_filter=aux.FALSE
function c56897896.synfilter(c,syncard,tuner,f)
	return c:IsFaceup() and c:IsNotTuner() and c:IsCanBeSynchroMaterial(syncard,tuner)
		and c:IsLevelBelow(4) and c:IsRace(RACE_WARRIOR+RACE_MACHINE) and (f==nil or f(c))
end
function c56897896.syntg(e,syncard,f,minc,maxc)
	local c=e:GetHandler()
	local lv=syncard:GetLevel()-c:GetLevel()
	if lv<=0 then return false end
	local g=Duel.GetMatchingGroup(c56897896.synfilter,syncard:GetControler(),LOCATION_MZONE,LOCATION_MZONE,c,syncard,c,f)
	return lv>=minc and lv<=maxc and g:GetCount()>=lv
end
function c56897896.synop(e,tp,eg,ep,ev,re,r,rp,syncard,f,minc,maxc)
	local c=e:GetHandler()
	local lv=syncard:GetLevel()-c:GetLevel()
	local g=Duel.GetMatchingGroup(c56897896.synfilter,syncard:GetControler(),LOCATION_MZONE,LOCATION_MZONE,c,syncard,c,f)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SMATERIAL)
	local sg=g:Select(tp,lv,lv,nil)
	Duel.SetSynchroMaterial(sg)
end
