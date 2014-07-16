--ゴーストリック・スペクター
function c81907872.initial_effect(c)
	--summon limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_SUMMON)
	e1:SetCondition(c81907872.sumcon)
	c:RegisterEffect(e1)
	--turn set
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(81907872,0))
	e2:SetCategory(CATEGORY_POSITION)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTarget(c81907872.postg)
	e2:SetOperation(c81907872.posop)
	c:RegisterEffect(e2)
	--spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(81907872,1))
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_DRAW)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetRange(LOCATION_HAND)
	e3:SetCondition(c81907872.spcon)
	e3:SetTarget(c81907872.sptg)
	e3:SetOperation(c81907872.spop)
	c:RegisterEffect(e3)
end
function c81907872.sfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x8d)
end
function c81907872.sumcon(e)
	return not Duel.IsExistingMatchingCard(c81907872.sfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c81907872.postg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsCanTurnSet() and c:GetFlagEffect(81907872)==0 end
	c:RegisterFlagEffect(81907872,RESET_EVENT+0x1fc0000+RESET_PHASE+PHASE_END,0,1)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,c,1,0,0)
end
function c81907872.posop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		Duel.ChangePosition(c,POS_FACEDOWN_DEFENCE)
	end
end
function c81907872.cfilter(c,tp)
	return c:IsControler(tp) and c:GetPreviousControler()==tp and c:IsSetCard(0x8d) and c:IsReason(REASON_DESTROY) and c:IsType(TYPE_MONSTER)
		and (not c:IsReason(REASON_BATTLE) or c==Duel.GetAttackTarget())
end
function c81907872.spcon(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp and eg:IsExists(c81907872.cfilter,1,nil,tp)
end
function c81907872.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN)
		and Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c81907872.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEDOWN_DEFENCE)~=0 then
		Duel.ConfirmCards(1-tp,c)
		Duel.Draw(tp,1,REASON_EFFECT)
	end
end
