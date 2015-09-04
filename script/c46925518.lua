--ゴーストリックの人形
function c46925518.initial_effect(c)
	--summon limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_SUMMON)
	e1:SetCondition(c46925518.sumcon)
	c:RegisterEffect(e1)
	--turn set
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(46925518,0))
	e2:SetCategory(CATEGORY_POSITION)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTarget(c46925518.postg)
	e2:SetOperation(c46925518.posop)
	c:RegisterEffect(e2)
	--
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_FLIP)
	e3:SetOperation(c46925518.fdop)
	c:RegisterEffect(e3)
end
function c46925518.sfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x8d)
end
function c46925518.sumcon(e)
	return not Duel.IsExistingMatchingCard(c46925518.sfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c46925518.postg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsCanTurnSet() and c:GetFlagEffect(46925518)==0 end
	c:RegisterFlagEffect(46925518,RESET_EVENT+0x1fc0000+RESET_PHASE+PHASE_END,0,1)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,c,1,0,0)
end
function c46925518.posop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		Duel.ChangePosition(c,POS_FACEDOWN_DEFENCE)
	end
end
function c46925518.fdop(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetCondition(c46925518.condition)
	e1:SetOperation(c46925518.operation)
	e1:SetReset(RESET_PHASE+RESET_END)
	Duel.RegisterEffect(e1,tp)
end
function c46925518.filter(c)
	return c:IsFaceup() and c:IsCanTurnSet()
end
function c46925518.spfilter(c,e,tp,lv)
	return c:IsSetCard(0x8d) and c:IsLevelBelow(lv) and c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEDOWN)
end
function c46925518.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c46925518.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil)
end
function c46925518.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c46925518.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if g:GetCount()==0 then return end
	local ct=Duel.ChangePosition(g,POS_FACEDOWN_DEFENCE)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local sg=Duel.GetMatchingGroup(c46925518.spfilter,tp,LOCATION_DECK,0,nil,e,tp,ct)
	if sg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(46925518,1)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local tg=sg:Select(tp,1,1,nil)
		if Duel.SpecialSummon(tg,0,tp,tp,false,false,POS_FACEDOWN_DEFENCE)~=0 then
			Duel.ConfirmCards(1-tp,tg)
		end
	end
end
