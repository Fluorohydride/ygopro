--Dragoon D－END
function c76263644.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	Auxiliary.AddFusionProcCode2(c,83965310,17132130,false,false)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCategory(CATEGORY_DAMAGE+CATEGORY_DESTROY)
	e2:SetDescription(aux.Stringid(76263644,0))
	e2:SetCountLimit(1)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c76263644.descondition)
	e2:SetTarget(c76263644.destarget)
	e2:SetOperation(c76263644.desoperation)
	c:RegisterEffect(e2)
	--Special Summon
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_TRIGGER_O+EFFECT_TYPE_FIELD)
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetRange(LOCATION_GRAVE)
	e3:SetDescription(aux.Stringid(76263644,1))
	e3:SetCondition(c76263644.spcondition)
	e3:SetCost(c76263644.spcost)
	e3:SetTarget(c76263644.sptarget)
	e3:SetOperation(c76263644.spoperation)
	c:RegisterEffect(e3)
	c:EnableReviveLimit()
end
c76263644.material_count=2
c76263644.material={83965310,17132130}
function c76263644.fscondition(e,g,gc)
	if g==nil then return false end
	if gc then return (gc:IsCode(83965310) and g:IsExists(Card.IsCode,1,nil,17132130))
		or (gc:IsCode(17132130) and g:IsExists(Card.IsCode,1,nil,83965310)) end
	return g:IsExists(Card.IsCode,1,nil,83965310) and g:IsExists(Card.IsCode,1,nil,17132130)
end
function c76263644.fsoperation(e,tp,eg,ep,ev,re,r,rp,gc)
	if gc then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		if gc:IsCode(83965310) then Duel.SetFusionMaterial(eg:FilterSelect(tp,Card.IsCode,1,1,nil,17132130))
		else Duel.SetFusionMaterial(eg:FilterSelect(tp,Card.IsCode,1,1,nil,83965310)) end
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		local g1=eg:FilterSelect(tp,Card.IsCode,1,1,nil,83965310)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
		local g2=eg:FilterSelect(tp,Card.IsCode,1,1,nil,17132130)
		g1:Merge(g2)
		Duel.SetFusionMaterial(g1)
	end
end
function c76263644.descondition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()==PHASE_MAIN1
end
function c76263644.destarget(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_MZONE,1,1,nil)
	local d=g:GetFirst()
	local atk=0
	if d:IsFaceup() then atk=d:GetAttack() end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,atk)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_BP)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetTargetRange(1,0)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c76263644.desoperation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local atk=0
		if tc:IsFaceup() then atk=tc:GetAttack() end
		if Duel.Destroy(tc,REASON_EFFECT)==0 then return end
		Duel.Damage(1-tp,atk,REASON_EFFECT)
	end
end
function c76263644.spcondition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp and Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c76263644.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c76263644.spfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c76263644.spfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c76263644.spfilter(c)
	return c:IsSetCard(0xc008) and c:IsAbleToRemoveAsCost()
end
function c76263644.sptarget(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c76263644.spoperation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
