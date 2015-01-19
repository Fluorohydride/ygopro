--ターボ・シンクロン
function c67270095.initial_effect(c)
	--pos
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(67270095,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetTarget(c67270095.postg)
	e1:SetOperation(c67270095.posop)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(67270095,1))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EVENT_DAMAGE)
	e2:SetCondition(c67270095.spcon)
	e2:SetTarget(c67270095.sptg)
	e2:SetOperation(c67270095.spop)
	c:RegisterEffect(e2)
end
function c67270095.postg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local d=Duel.GetAttackTarget()
	if chkc then return chkc==d end
	if chk==0 then return d and d:IsAttackPos() and d:IsCanBeEffectTarget(e) end
	Duel.SetTargetCard(d)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,d,1,0,0)
end
function c67270095.posop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and tc:IsAttackPos() then
		Duel.ChangePosition(tc,POS_FACEUP_DEFENCE)
	end
end
function c67270095.spcon(e,tp,eg,ep,ev,re,r,rp)
	return r==REASON_BATTLE and ep==tp and Duel.GetAttacker()==e:GetHandler()
end
function c67270095.filter(c,e,tp,dam)
	return c:IsAttackBelow(dam) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c67270095.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c67270095.filter,tp,LOCATION_HAND,0,1,nil,e,tp,ev) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c67270095.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c67270095.filter,tp,LOCATION_HAND,0,1,1,nil,e,tp,ev)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
