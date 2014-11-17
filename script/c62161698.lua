--イリュージョン·バルーン
function c62161698.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(62161698,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCondition(c62161698.condition)
	e1:SetTarget(c62161698.target)
	e1:SetOperation(c62161698.activate)
	c:RegisterEffect(e1)
	if not c62161698.global_check then
		c62161698.global_check=true
		c62161698[0]=false
		c62161698[1]=false
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_DESTROY)
		ge1:SetOperation(c62161698.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c62161698.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c62161698.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		if tc:IsLocation(LOCATION_MZONE) then
			c62161698[tc:GetControler()]=true
		end
		tc=eg:GetNext()
	end
end
function c62161698.clear(e,tp,eg,ep,ev,re,r,rp)
	c62161698[0]=false
	c62161698[1]=false
end
function c62161698.condition(e,tp,eg,ep,ev,re,r,rp)
	return c62161698[tp]
end
function c62161698.filter(c,e,tp)
	return c:IsSetCard(0x9f) and c:IsType(TYPE_MONSTER) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c62161698.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>=5 end
end
function c62161698.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)==0 then return end
	Duel.ConfirmDecktop(tp,5)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local g=Duel.GetDecktopGroup(tp,5)
	if g:IsExists(c62161698.filter,1,nil,e,tp) and Duel.SelectYesNo(tp,aux.Stringid(62161698,1)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=g:FilterSelect(tp,c62161698.filter,1,1,nil,e,tp)
		Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
		Duel.ShuffleDeck(tp)
	end
end
