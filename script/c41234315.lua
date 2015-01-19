--フェイク・エクスプロージョン・ペンタ
function c41234315.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c41234315.condition)
	e1:SetOperation(c41234315.activate)
	c:RegisterEffect(e1)
end
function c41234315.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetFirst():IsControler(1-tp)
end
function c41234315.activate(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if a then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
		e1:SetValue(1)
		e1:SetReset(RESET_PHASE+PHASE_DAMAGE)
		a:RegisterEffect(e1)
	end
	if d then
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
		e2:SetValue(1)
		e2:SetReset(RESET_PHASE+PHASE_DAMAGE)
		d:RegisterEffect(e2)
	end
	local e3=Effect.CreateEffect(e:GetHandler())
	e3:SetDescription(aux.Stringid(41234315,0))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_BATTLED)
	e3:SetOperation(c41234315.spop)
	e3:SetReset(RESET_PHASE+PHASE_DAMAGE)
	Duel.RegisterEffect(e3,tp)
end
function c41234315.spfilter(c,e,tp)
	return c:IsCode(89493368) and c:IsCanBeSpecialSummoned(e,0,tp,false,false) and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c41234315.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c41234315.spfilter,tp,0x12,0,1,1,nil,e,tp)
	if g:GetCount()~=0 then
		Duel.SpecialSummon(g,0,tp,tp,true,true,POS_FACEUP)
	end
end
