--PSYフレームギア・β
function c2810642.initial_effect(c)
	c:EnableUnsummonable()
	--splimit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(c2810642.splimit)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(2810642,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetRange(LOCATION_HAND)
	e2:SetCode(EVENT_ATTACK_ANNOUNCE)
	e2:SetCondition(c2810642.condition)
	e2:SetTarget(c2810642.target)
	e2:SetOperation(c2810642.operation)
	c:RegisterEffect(e2)
end
function c2810642.splimit(e,se,sp,st)
	return se:IsHasType(EFFECT_TYPE_ACTIONS)
end
function c2810642.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)==0 and Duel.GetAttacker():GetControler()~=tp
end
function c2810642.spfilter(c,e,tp)
	return c:IsCode(49036338) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c2810642.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>1
		and Duel.GetAttacker():IsDestructable()
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false)
		and Duel.IsExistingMatchingCard(c2810642.spfilter,tp,LOCATION_HAND+LOCATION_DECK+LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.SetTargetCard(Duel.GetAttacker())
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,2,tp,LOCATION_HAND+LOCATION_DECK+LOCATION_GRAVE)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,Duel.GetAttacker(),1,0,0)
end
function c2810642.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<2 or not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c2810642.spfilter,tp,LOCATION_HAND+LOCATION_DECK+LOCATION_GRAVE,0,1,1,nil,e,tp)
	if g:FilterCount(aux.nvfilter,nil)==0 then return end
	local tc=g:GetFirst()
	local c=e:GetHandler()
	local fid=c:GetFieldID()
	Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP)
	Duel.SpecialSummonStep(c,0,tp,tp,false,false,POS_FACEUP)
	tc:RegisterFlagEffect(2810642,RESET_EVENT+0x1fe0000,0,1,fid)
	c:RegisterFlagEffect(2810642,RESET_EVENT+0x1fe0000,0,1,fid)
	Duel.SpecialSummonComplete()
	g:AddCard(c)
	g:KeepAlive()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetLabel(fid)
	e1:SetLabelObject(g)
	e1:SetCondition(c2810642.rmcon)
	e1:SetOperation(c2810642.rmop)
	Duel.RegisterEffect(e1,tp)
	local dc=Duel.GetFirstTarget()
	if dc:IsRelateToEffect(e) and Duel.Destroy(dc,REASON_EFFECT)~=0 then
		Duel.BreakEffect()
		Duel.SkipPhase(1-tp,PHASE_BATTLE,RESET_PHASE+PHASE_BATTLE,1)
	end
end
function c2810642.rmfilter(c,fid)
	return c:GetFlagEffectLabel(2810642)==fid
end
function c2810642.rmcon(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	if not g:IsExists(c2810642.rmfilter,1,nil,e:GetLabel()) then
		g:DeleteGroup()
		e:Reset()
		return false
	else return true end
end
function c2810642.rmop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local tg=g:Filter(c2810642.rmfilter,nil,e:GetLabel())
	Duel.Remove(tg,POS_FACEUP,REASON_EFFECT)
end
