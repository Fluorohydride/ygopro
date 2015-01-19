--レアメタル・ナイト
function c1412158.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCode2(c,92421852,38916461,true,true)
	--atk up
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(1412158,0))
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetCondition(c1412158.atkcon)
	e1:SetValue(1000)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(1412158,1))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c1412158.spcon)
	e2:SetTarget(c1412158.sptg)
	e2:SetOperation(c1412158.spop)
	c:RegisterEffect(e2)
end
function c1412158.atkcon(e)
	local ph=Duel.GetCurrentPhase()
	if not (ph==PHASE_DAMAGE or ph==PHASE_DAMAGE_CAL) then return false end
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return a==e:GetHandler() and d~=nil
end
function c1412158.spcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetTurnID()~=Duel.GetTurnCount()
end
function c1412158.spfilter(c,e,tp)
	return c:IsCode(75923050) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c1412158.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and e:GetHandler():IsAbleToExtra()
		and Duel.IsExistingMatchingCard(c1412158.spfilter,tp,LOCATION_EXTRA,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_TODECK,e:GetHandler(),1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c1412158.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<0 then return end
	if c:IsRelateToEffect(e) and c:IsFaceup() and Duel.SendtoDeck(c,nil,2,REASON_EFFECT)~=0 then
		local tc=Duel.GetFirstMatchingCard(c1412158.spfilter,tp,LOCATION_EXTRA,0,nil,e,tp)
		if tc then
			Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
		end
	end
end
