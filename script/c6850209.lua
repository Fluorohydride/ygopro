--デーモンとの駆け引き
function c6850209.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c6850209.target)
	e1:SetOperation(c6850209.activate)
	c:RegisterEffect(e1)
	if not c6850209.global_check then
		c6850209.global_check=true
		c6850209[0]=false
		c6850209[1]=false
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_TO_GRAVE)
		ge1:SetOperation(c6850209.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c6850209.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c6850209.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		if tc:IsLevelAbove(8) and tc:IsPreviousLocation(LOCATION_ONFIELD) then
			c6850209[tc:GetPreviousControler()]=true
		end
		tc=eg:GetNext()
	end
end
function c6850209.clear(e,tp,eg,ep,ev,re,r,rp)
	c6850209[0]=false
	c6850209[1]=false
end
function c6850209.filter(c,e,tp)
	return c:IsCode(85605684) and c:IsCanBeSpecialSummoned(e,0,tp,true,false)
end
function c6850209.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return c6850209[tp] and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and
		Duel.IsExistingMatchingCard(c6850209.filter,tp,LOCATION_DECK+LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK+LOCATION_HAND)
end
function c6850209.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c6850209.filter,tp,LOCATION_DECK+LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,true,false,POS_FACEUP)
	end
end
