--地獄の暴走召喚
function c12247206.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c12247206.condition)
	e1:SetTarget(c12247206.target)
	e1:SetOperation(c12247206.activate)
	c:RegisterEffect(e1)
end
function c12247206.cfilter(c,tp)
	return c:IsLocation(LOCATION_MZONE) and c:IsControler(tp)
end
function c12247206.condition(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c12247206.cfilter,nil,tp)
	if g:GetCount()~=1 then return end
	local tc=g:GetFirst()
	e:SetLabelObject(tc)
	return tc:IsAttackBelow(1500) and Duel.IsExistingMatchingCard(Card.IsFaceup,tp,0,LOCATION_MZONE,1,nil)
end
function c12247206.filter(c,code,e,tp)
	return c:GetCode()==code and c:IsCanBeSpecialSummoned(e,0,tp,false,false) and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c12247206.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=e:GetLabelObject()
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c12247206.filter,tp,0x13,0,1,nil,tc:GetCode(),e,tp) end
	tc:CreateEffectRelation(e)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,PLAYER_ALL,0x13)
end
function c12247206.selfilter(c)
	return c:IsFaceup() and not c:IsType(TYPE_TOKEN+TYPE_TRAP)
end
function c12247206.rmfilter(c)
	return c:IsLocation(LOCATION_MZONE) or c:IsLocation(LOCATION_GRAVE)
end
function c12247206.sp(g,tp,pos)
	local sc=g:GetFirst()
	while sc do
		Duel.SpecialSummonStep(sc,0,tp,tp,false,false,pos)
		sc=g:GetNext()
	end
end
function c12247206.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	local ft1=Duel.GetLocationCount(tp,LOCATION_MZONE)
	local gg=Group.CreateGroup()
	if ft1>0 and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local g=Duel.GetMatchingGroup(c12247206.filter,tp,0x13,0,nil,tc:GetCode(),e,tp)
		if g:GetCount()<=ft1 then c12247206.sp(g,tp,POS_FACEUP_ATTACK)
		else
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			local fg=g:Select(tp,ft1,ft1,nil)
			c12247206.sp(fg,tp,POS_FACEUP_ATTACK)
			g:Remove(c12247206.rmfilter,nil)
			gg:Merge(g)
		end
	end
	local ft2=Duel.GetLocationCount(1-tp,LOCATION_MZONE)
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_FACEUP)
	local sg=Duel.SelectMatchingCard(1-tp,c12247206.selfilter,1-tp,LOCATION_MZONE,0,1,1,nil)
	if ft2>0 and sg:GetCount()>0 then
		local g=Duel.GetMatchingGroup(c12247206.filter,1-tp,0x13,0,nil,sg:GetFirst():GetCode(),e,1-tp)
		if g:GetCount()<=ft2 then c12247206.sp(g,1-tp,POS_FACEUP)
		else
			Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_SPSUMMON)
			local fg=g:Select(1-tp,ft2,ft2,nil)
			c12247206.sp(fg,1-tp,POS_FACEUP)
			g:Remove(c12247206.rmfilter,nil)
			gg:Merge(g)
		end
	end
	Duel.SpecialSummonComplete()
	Duel.SendtoGrave(gg,REASON_EFFECT)
end
