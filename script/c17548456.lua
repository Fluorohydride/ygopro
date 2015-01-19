--ジュラック・メテオ
function c17548456.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,aux.FilterBoolFunction(Card.IsSetCard,0x22),aux.NonTuner(Card.IsRace,RACE_DINOSAUR),2)
	c:EnableReviveLimit()
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(17548456,0))
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c17548456.descon)
	e1:SetTarget(c17548456.destg)
	e1:SetOperation(c17548456.desop)
	c:RegisterEffect(e1)
end
function c17548456.descon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_SYNCHRO
end
function c17548456.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c17548456.spfilter(c,e,tp)
	return c:IsType(TYPE_TUNER) and c:IsCanBeSpecialSummoned(e,0,tp,false,false) and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c17548456.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	if g:GetCount()>0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
	local sg=Duel.GetMatchingGroup(c17548456.spfilter,tp,LOCATION_GRAVE,0,nil,e,tp)
	if sg:GetCount()~=0 and Duel.SelectYesNo(tp,aux.Stringid(17548456,1)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sp=sg:Select(tp,1,1,nil)
		Duel.SpecialSummon(sp,0,tp,tp,false,false,POS_FACEUP)
	end
end
